--[[
  Quest 841: Curse of the Black Widow
  Level Required: 85
  NPC Start: 126, NPC Complete: 128
]]

local fsm = require('fsm_engine')

local quest_841 = fsm:new({
    id = 841,
    name = "Curse of the Black Widow",
    level_required = 85,
    npc_start = 126,
    npc_complete = 128,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 841: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 841: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 841: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 841: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 126,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 841: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 841: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 841: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 841: Rewards given")
                self:give_rewards({
                })
            end,
            transitions = {},
        },
    },
})

return quest_841