--[[
  Quest 186: Next Stop: Nera
  Level Required: 38
  NPC Start: 9, NPC Complete: 10
  Prerequisites: [185]
  Rewards: EXP=51033, Gold=500
]]

local fsm = require('fsm_engine')

local quest_186 = fsm:new({
    id = 186,
    name = "Next Stop: Nera",
    level_required = 38,
    npc_start = 9,
    npc_complete = 10,
    prerequisites = {185},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 186: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 186: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 186: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 186: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 186: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 186: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 186: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 186: Rewards given")
                self:give_rewards({
                    exp = 51033,
                    gold = 500,
                    items = {
                        { item_id = 21000097, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_186