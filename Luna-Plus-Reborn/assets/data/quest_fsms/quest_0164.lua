--[[
  Quest 164: Arms Race
  Level Required: 35
  NPC Start: 1, NPC Complete: 1
  Rewards: EXP=74416, Gold=4560
]]

local fsm = require('fsm_engine')

local quest_164 = fsm:new({
    id = 164,
    name = "Arms Race",
    level_required = 35,
    npc_start = 1,
    npc_complete = 1,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 164: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 1,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 164: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 164: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 51,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 164: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 56,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 164: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 57,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 164: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 164: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 164: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 1,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 164: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 164: Rewards given")
                self:give_rewards({
                    exp = 74416,
                    gold = 4560,
                    items = {
                        { item_id = 80, count = 1 },
                        { item_id = 81, count = 1 },
                        { item_id = 82, count = 1 },
                        { item_id = 80, count = 1 },
                        { item_id = 81, count = 1 },
                        { item_id = 82, count = 1 },
                        { item_id = 30000826, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_164