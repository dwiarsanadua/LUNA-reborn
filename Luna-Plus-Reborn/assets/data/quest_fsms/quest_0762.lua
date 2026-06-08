--[[
  Quest 762: [Quest] Blue Land
  Level Required: 0
  NPC Start: 9, NPC Complete: 400
  Prerequisites: [761]
  Rewards: EXP=50000, Gold=10000
]]

local fsm = require('fsm_engine')

local quest_762 = fsm:new({
    id = 762,
    name = "[Quest] Blue Land",
    level_required = 0,
    npc_start = 9,
    npc_complete = 400,
    prerequisites = {761},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 762: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 400,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 762: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 762: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 400,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 762: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 762: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 762: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 400,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 762: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 762: Rewards given")
                self:give_rewards({
                    exp = 50000,
                    gold = 10000,
                    items = {
                        { item_id = 21001530, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_762