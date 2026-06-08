--[[
  Quest 732: [Quest] Help two of the Top Union
  Level Required: 54
  NPC Start: 117, NPC Complete: 117
  Prerequisites: [731]
  Rewards: EXP=414585
]]

local fsm = require('fsm_engine')

local quest_732 = fsm:new({
    id = 732,
    name = "[Quest] Help two of the Top Union",
    level_required = 54,
    npc_start = 117,
    npc_complete = 117,
    prerequisites = {731},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 732: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 732: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 732: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 62,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 732: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 732: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 732: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 732: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 732: Rewards given")
                self:give_rewards({
                    exp = 414585,
                })
            end,
            transitions = {},
        },
    },
})

return quest_732