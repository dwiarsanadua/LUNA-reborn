--[[
  Quest 436: [Quest] Competitor
  Level Required: 53
  NPC Start: 117, NPC Complete: 117
  Prerequisites: [435]
]]

local fsm = require('fsm_engine')

local quest_436 = fsm:new({
    id = 436,
    name = "[Quest] Competitor",
    level_required = 53,
    npc_start = 117,
    npc_complete = 117,
    prerequisites = {435},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 436: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 436: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 436: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 436: NPC talk objective met")
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
                self:log("Quest 436: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 436: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 436: Rewards given")
                self:give_rewards({
                })
            end,
            transitions = {},
        },
    },
})

return quest_436