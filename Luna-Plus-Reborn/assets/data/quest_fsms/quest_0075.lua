--[[
  Quest 75: Missing Teddy Bear
  Level Required: 4
  NPC Start: 8, NPC Complete: 8
  Rewards: EXP=112, Gold=1005
]]

local fsm = require('fsm_engine')

local quest_75 = fsm:new({
    id = 75,
    name = "Missing Teddy Bear",
    level_required = 4,
    npc_start = 8,
    npc_complete = 8,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 75: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 8,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 75: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 75: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 23,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 75: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 8,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 75: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 75: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 8,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 75: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 75: Rewards given")
                self:give_rewards({
                    exp = 112,
                    gold = 1005,
                    items = {
                        { item_id = 24, count = 1 },
                        { item_id = 24, count = 1 },
                        { item_id = 21000006, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_75