--[[
  Quest 406: [Special] Sealing The Demon Dragon
  Level Required: 50
  NPC Start: 9, NPC Complete: 96
  Prerequisites: [405]
]]

local fsm = require('fsm_engine')

local quest_406 = fsm:new({
    id = 406,
    name = "[Special] Sealing The Demon Dragon",
    level_required = 50,
    npc_start = 9,
    npc_complete = 96,
    prerequisites = {405},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 406: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 406: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 406: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 9,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 406: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 96,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 406: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 406: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 96,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 406: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 406: Rewards given")
                self:give_rewards({
                })
            end,
            transitions = {},
        },
    },
})

return quest_406