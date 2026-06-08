--[[
  Quest 846: Insane wreck
  Level Required: 86
  NPC Start: 505, NPC Complete: 505
  Prerequisites: [845]
  Rewards: EXP=3321421, Gold=57826
]]

local fsm = require('fsm_engine')

local quest_846 = fsm:new({
    id = 846,
    name = "Insane wreck",
    level_required = 86,
    npc_start = 505,
    npc_complete = 505,
    prerequisites = {845},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 846: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 505,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 846: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 846: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 685,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 846: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 505,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 846: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 846: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 505,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 846: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 846: Rewards given")
                self:give_rewards({
                    exp = 3321421,
                    gold = 57826,
                })
            end,
            transitions = {},
        },
    },
})

return quest_846