--[[
  Quest 82: [Repeatable] Zank's Second Request
  Level Required: 12
  NPC Start: 1, NPC Complete: 51
  Rewards: EXP=2143, Gold=1585
]]

local fsm = require('fsm_engine')

local quest_82 = fsm:new({
    id = 82,
    name = "[Repeatable] Zank's Second Request",
    level_required = 12,
    npc_start = 1,
    npc_complete = 51,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 82: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 51,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 82: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 82: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 8,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 82: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 31,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 82: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 51,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 82: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 82: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 82: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 51,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 82: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 82: Rewards given")
                self:give_rewards({
                    exp = 2143,
                    gold = 1585,
                    items = {
                        { item_id = 31, count = 1 },
                        { item_id = 31, count = 1 },
                        { item_id = 31, count = 10 },
                        { item_id = 21000007, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_82