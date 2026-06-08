--[[
  Quest 81: [Repeatable] Wiff's Request
  Level Required: 9
  NPC Start: 10, NPC Complete: 51
  Rewards: EXP=789, Gold=1508
]]

local fsm = require('fsm_engine')

local quest_81 = fsm:new({
    id = 81,
    name = "[Repeatable] Wiff's Request",
    level_required = 9,
    npc_start = 10,
    npc_complete = 51,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 81: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 51,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 81: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 81: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 5,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 81: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 29,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 81: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 51,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 81: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 81: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 81: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 51,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 81: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 81: Rewards given")
                self:give_rewards({
                    exp = 789,
                    gold = 1508,
                    items = {
                        { item_id = 30, count = 10 },
                        { item_id = 21000006, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_81