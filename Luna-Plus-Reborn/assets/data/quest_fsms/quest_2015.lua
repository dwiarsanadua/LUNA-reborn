--[[
  Quest 2015: [Daily] Ogre Subjugation
  Level Required: 65
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=390972
]]

local fsm = require('fsm_engine')

local quest_2015 = fsm:new({
    id = 2015,
    name = "[Daily] Ogre Subjugation",
    level_required = 65,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2015: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2015: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2015: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 40,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2015: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2015: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2015: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2015: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2015: Rewards given")
                self:give_rewards({
                    exp = 390972,
                    items = {
                        { item_id = 30000835, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2015