--[[
  Quest 83: [Repeatable] Farouk's Request
  Level Required: 15
  NPC Start: 7, NPC Complete: 51
  Rewards: EXP=4501, Gold=1485
]]

local fsm = require('fsm_engine')

local quest_83 = fsm:new({
    id = 83,
    name = "[Repeatable] Farouk's Request",
    level_required = 15,
    npc_start = 7,
    npc_complete = 51,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 83: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 51,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 83: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 83: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 19,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 83: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 51,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 83: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 83: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 83: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 51,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 83: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 83: Rewards given")
                self:give_rewards({
                    exp = 4501,
                    gold = 1485,
                    items = {
                        { item_id = 32, count = 1 },
                        { item_id = 32, count = 15 },
                        { item_id = 21000007, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_83