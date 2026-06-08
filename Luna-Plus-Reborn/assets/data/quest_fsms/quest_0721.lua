--[[
  Quest 721: What are you?
  Level Required: 0
  NPC Start: 119, NPC Complete: 119
  Prerequisites: [720]
  Rewards: EXP=437964
]]

local fsm = require('fsm_engine')

local quest_721 = fsm:new({
    id = 721,
    name = "What are you?",
    level_required = 0,
    npc_start = 119,
    npc_complete = 119,
    prerequisites = {720},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 721: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 721: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 721: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 395,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 721: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 721: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 721: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 721: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 721: Rewards given")
                self:give_rewards({
                    exp = 437964,
                })
            end,
            transitions = {},
        },
    },
})

return quest_721