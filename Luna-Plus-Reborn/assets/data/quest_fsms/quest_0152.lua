--[[
  Quest 152: Destination: Zakandia
  Level Required: 15
  NPC Start: 15, NPC Complete: 57
  Prerequisites: [151, 99, 134]
  Rewards: EXP=1957, Gold=530
]]

local fsm = require('fsm_engine')

local quest_152 = fsm:new({
    id = 152,
    name = "Destination: Zakandia",
    level_required = 15,
    npc_start = 15,
    npc_complete = 57,
    prerequisites = {151, 99, 134},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 152: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 152: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 152: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 152: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 57,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 152: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 152: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 57,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 152: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 152: Rewards given")
                self:give_rewards({
                    exp = 1957,
                    gold = 530,
                    items = {
                        { item_id = 21000007, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_152