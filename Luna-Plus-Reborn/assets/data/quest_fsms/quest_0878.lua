--[[
  Quest 878: Return to Nera Castle
  Level Required: 94
  NPC Start: 90, NPC Complete: 129
  Rewards: EXP=3055560
]]

local fsm = require('fsm_engine')

local quest_878 = fsm:new({
    id = 878,
    name = "Return to Nera Castle",
    level_required = 94,
    npc_start = 90,
    npc_complete = 129,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 878: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 878: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 878: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 878: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 124,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 878: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 129,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 878: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 878: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 129,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 878: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 878: Rewards given")
                self:give_rewards({
                    exp = 3055560,
                })
            end,
            transitions = {},
        },
    },
})

return quest_878