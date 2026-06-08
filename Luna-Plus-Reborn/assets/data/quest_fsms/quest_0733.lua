--[[
  Quest 733: [Quest] Do you give a…
  Level Required: 55
  NPC Start: 117, NPC Complete: 117
  Prerequisites: [732]
  Rewards: EXP=451316
]]

local fsm = require('fsm_engine')

local quest_733 = fsm:new({
    id = 733,
    name = "[Quest] Do you give a…",
    level_required = 55,
    npc_start = 117,
    npc_complete = 117,
    prerequisites = {732},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 733: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 733: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 733: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 394,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 733: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 733: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 733: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 733: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 733: Rewards given")
                self:give_rewards({
                    exp = 451316,
                })
            end,
            transitions = {},
        },
    },
})

return quest_733