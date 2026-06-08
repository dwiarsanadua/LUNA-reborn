--[[
  Quest 917: Serie's Still Crying
  Level Required: 83
  NPC Start: 124, NPC Complete: 124
  Prerequisites: [916]
  Rewards: EXP=2717141
]]

local fsm = require('fsm_engine')

local quest_917 = fsm:new({
    id = 917,
    name = "Serie's Still Crying",
    level_required = 83,
    npc_start = 124,
    npc_complete = 124,
    prerequisites = {916},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 917: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 124,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 917: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 917: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 278,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 917: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 124,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 917: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 917: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 124,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 917: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 917: Rewards given")
                self:give_rewards({
                    exp = 2717141,
                })
            end,
            transitions = {},
        },
    },
})

return quest_917