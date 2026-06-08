--[[
  Quest 606: Quest 606
  Level Required: 60
  NPC Start: 400, NPC Complete: 400
]]

local fsm = require('fsm_engine')

local quest_606 = fsm:new({
    id = 606,
    name = "Quest 606",
    level_required = 60,
    npc_start = 400,
    npc_complete = 400,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 606: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 400,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 606: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 606: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 400,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 606: NPC talk objective met")
                    end
                },
                {
                    trigger = "any",
                    target = "COMPLETE",
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 606: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 400,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 606: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 606: Rewards given")
                self:give_rewards({
                })
            end,
            transitions = {},
        },
    },
})

return quest_606